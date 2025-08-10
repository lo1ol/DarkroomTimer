import sys

# Переопределяем команду запуска теста
Import("env")

def run_test_with_valgrind(source, target, env):
    program_path = target[0].get_abspath()
    return env.Execute(f"valgrind --leak-check=full --error-exitcode=1 {program_path}")

env.AddPostAction("$PROGPATH", run_test_with_valgrind)
