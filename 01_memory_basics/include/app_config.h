/**
 * @file    app_config.h
 *
 * @brief   Project-wide compile-time configuration constants.
 *
 * @details Keeping all magic numbers here means a single file change tunes
 *          the entire application. Never scatter numeric literals across
 *          source files — reviewers and maintainers will thank you.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/** Number of iterations used in each demonstration loop. */
#define DEMO_ITERATIONS     (6U)

/** Console separator line width in characters. */
#define SEPARATOR_WIDTH     (65U)

/** Tick threshold at which the FSM demo transitions RUNNING -> FAULT. */
#define FSM_FAULT_TICK      (4U)

#endif /* APP_CONFIG_H */

/* End of file - app_config.h */
