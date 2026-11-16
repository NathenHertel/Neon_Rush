#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

class SceneManager;

class Scene
{
public:
    virtual ~Scene() = default;

    // Lifecycle. NOTE: `scenes` is not set yet inside the constructor,
    // so push/pop from OnEnter() or Update(), never from the constructor.
    virtual void OnEnter() {}    // scene became part of the stack
    virtual void OnExit() {}     // scene removed from the stack
    virtual void OnPause() {}    // another scene was pushed on top
    virtual void OnResume() {}   // the scene above was popped

    virtual void Update(float dt) = 0;          // only the TOP scene gets this
    virtual void UpdateCovered(float dt) {}     // scenes UNDER the top get this (e.g. keep particles animating)
    virtual void Render() = 0;

    // true  -> scene underneath is still drawn (pause menu, level up, game over)
    // false -> scene hides everything below it
    virtual bool IsOverlay() const { return false; }

protected:
    SceneManager* scenes = nullptr;

private:
    friend class SceneManager;
};

class SceneManager
{
    enum class Op { Push, Pop, Replace, Reset };
    struct Command { Op op; std::unique_ptr<Scene> scene; };

    std::vector<std::unique_ptr<Scene>> stack;
    std::vector<Command> pending;

public:
    SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    // ---- Requests (safe to call from inside a scene's Update) ----
    // Applied at the end of the frame, so a scene is never destroyed while it is running.

    template<class T, class... Args>
    void Push(Args&&... args) { Queue(Op::Push, std::make_unique<T>(std::forward<Args>(args)...)); }

    template<class T, class... Args>
    void Replace(Args&&... args) { Queue(Op::Replace, std::make_unique<T>(std::forward<Args>(args)...)); }

    template<class T, class... Args>
    void Reset(Args&&... args) { Queue(Op::Reset, std::make_unique<T>(std::forward<Args>(args)...)); }

    void Pop() { Queue(Op::Pop, nullptr); }

    // ---- Per-frame ----

    void Update(float dt)
    {
        for (std::size_t i = 0; i + 1 < stack.size(); ++i)
            stack[i]->UpdateCovered(dt);

        if (!stack.empty())
            stack.back()->Update(dt);

        Flush();
    }

    void Render()
    {
        // Walk down from the top until we hit a scene that isn't an overlay.
        std::size_t first = stack.size();
        while (first > 0)
        {
            --first;
            if (!stack[first]->IsOverlay()) break;
        }
        for (std::size_t i = first; i < stack.size(); ++i)
            stack[i]->Render();
    }

    // Apply queued changes. Update() calls this; call it manually once after the first Push/Reset.
    void Flush()
    {
        while (!pending.empty())
        {
            // Swap out first: OnEnter()/OnResume() may queue more commands.
            std::vector<Command> batch = std::move(pending);
            pending.clear();

            for (auto& c : batch)
            {
                switch (c.op)
                {
                case Op::Push:
                    if (!stack.empty()) stack.back()->OnPause();
                    Attach(std::move(c.scene));
                    break;

                case Op::Pop:
                    PopTop();
                    if (!stack.empty()) stack.back()->OnResume();
                    break;

                case Op::Replace:
                    PopTop();
                    Attach(std::move(c.scene));
                    break;

                case Op::Reset:
                    while (!stack.empty()) PopTop();
                    Attach(std::move(c.scene));
                    break;
                }
            }
        }
    }

    Scene* Top() const { return stack.empty() ? nullptr : stack.back().get(); }
    bool   Empty() const { return stack.empty(); }

private:
    void Queue(Op op, std::unique_ptr<Scene> s) { pending.push_back(Command{ op, std::move(s) }); }

    void Attach(std::unique_ptr<Scene> s)
    {
        s->scenes = this;
        stack.push_back(std::move(s));
        stack.back()->OnEnter();
    }

    void PopTop()
    {
        if (stack.empty()) return;
        stack.back()->OnExit();
        stack.pop_back();
    }
};