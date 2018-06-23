import pylint.lint



PYLINT_OPTIONS = [
    "app/",
    "common/",
    "jetson_io/",
    "server/",
    "tasks/",
]

pylint.lint.Run(PYLINT_OPTIONS)
