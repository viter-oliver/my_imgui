//
// detail/timer_scheduler_fwd.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_DETAIL_TIMER_SCHEDULER_FWD_HPP
#define ASIO_DETAIL_TIMER_SCHEDULER_FWD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "RCF/external/asio/asio/detail/config.hpp"

#if defined(ASIO_HAS_IOCP)
# include "RCF/external/asio/asio/detail/win_iocp_io_service_fwd.hpp"
#elif defined(ASIO_HAS_EPOLL)
# include "RCF/external/asio/asio/detail/epoll_reactor_fwd.hpp"
#elif defined(ASIO_HAS_KQUEUE)
# include "RCF/external/asio/asio/detail/kqueue_reactor_fwd.hpp"
#elif defined(ASIO_HAS_DEV_POLL)
# include "RCF/external/asio/asio/detail/dev_poll_reactor_fwd.hpp"
#else
# include "RCF/external/asio/asio/detail/select_reactor_fwd.hpp"
#endif

namespace asio {
namespace detail {

#if defined(ASIO_HAS_IOCP)
typedef win_iocp_io_service timer_scheduler;
#elif defined(ASIO_HAS_EPOLL)
typedef epoll_reactor timer_scheduler;
#elif defined(ASIO_HAS_KQUEUE)
typedef kqueue_reactor timer_scheduler;
#elif defined(ASIO_HAS_DEV_POLL)
typedef dev_poll_reactor timer_scheduler;
#else
typedef select_reactor timer_scheduler;
#endif

} // namespace detail
} // namespace asio

#endif // ASIO_DETAIL_TIMER_SCHEDULER_FWD_HPP
