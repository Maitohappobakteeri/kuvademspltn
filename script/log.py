from pprint import pformat
from termcolor import colored
from shutil import get_terminal_size
from enum import Enum
import math
import time


class LogTypes(Enum):
    IMPORTANT = 0
    NORMAL = 1
    WARNING = 2
    ERROR = 3
    DATA = 4


class LogColors(Enum):
    DEFAULT = ("light_magenta",)
    HIGHLIGHT = ("blue", "on_light_cyan")
    WARNING = ("yellow", "on_dark_grey")
    ERROR = ("black", "on_light_red")
    DATA = ("magenta", "on_dark_grey")


class ProgressStatus:
    def __init__(self, max_steps):
        self.max_steps = max_steps
        self.step = 0

    def format(self, message, max_width, no_step=False):
        progress_bar_width = max(min(max_width * 0.2, 5), 30)
        message_width = max_width - progress_bar_width - 1 - 2

        full_hearts = math.floor(progress_bar_width * (self.step / self.max_steps))
        empty_hearts = progress_bar_width - full_hearts
        progress_bar = "[" + "♡" * full_hearts + " " * empty_hearts + "]"

        if not no_step:
            self.step += 1
            self.step = min(self.step, self.max_steps)

        return f"{progress_bar} {message.ljust(message_width)[:message_width]}"

    def on_done(self, max_width):
        progress_bar_width = math.floor(max(min(max_width * 0.2, 5), 30))
        message_width = max_width - progress_bar_width - 1 - 2
        message = "done"
        full_hearts = progress_bar_width
        empty_hearts = 0
        progress_bar = "[" + "♡" * full_hearts + " " * empty_hearts + "]"
        return colored(
            f"{progress_bar} {message.ljust(message_width)[:message_width]}",
            *LogColors.DEFAULT.value,
        )


def pretty_format(obj):
    return pformat(obj)


was_repeating_status = False
repeating_status_animation = 0
status_state = None
prev_status_state = None
prev_time = time.time()


def set_status_state(state):
    global status_state
    status_state = state


def log(
    message,
    type=LogTypes.NORMAL,
    repeating_status=False,
    no_step=False,
    multiline=False,
):
    global was_repeating_status, repeating_status_animation, status_state, prev_status_state, prev_time
    term_width = get_terminal_size((100, 20))[0]
    repeating_status_animation += 1

    if repeating_status:
        loading_icons = ["|", "/", "—", "\\"]
        loading_icon = loading_icons[repeating_status_animation % len(loading_icons)]
        message = f"{loading_icon}  {message}"

    color = LogColors.DEFAULT

    if type is LogTypes.IMPORTANT:
        message = f"❤  {message}"
        color = LogColors.HIGHLIGHT
    elif type is LogTypes.WARNING:
        message = f"⚠  {message}"
        color = LogColors.WARNING
    elif type is LogTypes.ERROR:
        message = f"⚠  {message}"
        color = LogColors.ERROR
    elif type is LogTypes.DATA:
        color = LogColors.DATA

    lines = message.splitlines()
    lines = [
        line if len(line) > 0 else " "
        for line in lines
    ]
    if multiline:
        lines = [
            line[i : i + term_width]
            for line in lines
            for i in range(0, len(line), term_width)
        ]
    padded_lines = [line.ljust(term_width)[:term_width] for line in lines]
    formatted = "\n".join(padded_lines)

    if repeating_status:
        if time.time() - prev_time < 0.1:
            return

        if status_state:
            if prev_status_state is not status_state and prev_status_state is not None:
                print(f"\r{status_state.on_done(term_width)}")
            prev_status_state = status_state
            formatted = status_state.format(formatted, term_width, no_step=no_step)
        formatted = f"\r{formatted}"
    else:
        if status_state and type not in [LogTypes.WARNING, LogTypes.ERROR]:
            print(f"\r{status_state.on_done(term_width)}")
            status_state = None
        elif was_repeating_status:
            print("")
    print(colored(formatted, *color.value), end="" if repeating_status else "\n")
    prev_time = time.time()
    was_repeating_status = repeating_status


def important(*args, **kwargs):
    log(*args, type=LogTypes.IMPORTANT, **kwargs)


def warning(*args, **kwargs):
    log(*args, type=LogTypes.WARNING, **kwargs)


def error(*args, **kwargs):
    log(*args, type=LogTypes.ERROR, **kwargs)
