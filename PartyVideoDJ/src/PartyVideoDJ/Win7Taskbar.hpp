//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file Win7Taskbar.hpp Windows 7 task bar access
//
#pragma once

namespace Win7
{
   class TaskbarImpl;

   class TaskbarProgressBar
   {
   public:
      enum TaskbarProgressBarState
      {
         TBPF_NOPROGRESS = 0,
         TBPF_INDETERMINATE = 0x1,
         TBPF_NORMAL = 0x2,
         TBPF_ERROR = 0x4,
         TBPF_PAUSED = 0x8
      };

      ~TaskbarProgressBar()
      {
         SetState(TBPF_NOPROGRESS);
      }

      void SetState(TaskbarProgressBarState enState);
      void SetPos(UINT uiPos, UINT uiMax);

   private:
      friend class Taskbar;

      TaskbarProgressBar(std::shared_ptr<TaskbarImpl> spImpl)
         :m_spImpl(spImpl)
      {
         SetState(TBPF_INDETERMINATE);
      }

   private:
      std::shared_ptr<TaskbarImpl> m_spImpl;
   };

   /// Windows 7 taskbar access
   class Taskbar
   {
   public:
      Taskbar(HWND hWnd) throw();

      bool IsAvailable() const throw();

      ~Taskbar() throw()
      {
      }

      TaskbarProgressBar OpenProgressBar()
      {
         ATLASSERT(IsAvailable());
         return TaskbarProgressBar(m_spImpl);
      }

   private:
      std::shared_ptr<TaskbarImpl> m_spImpl;
   };

} // namespace Win7
