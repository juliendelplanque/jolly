import sys
import os

sys.path.insert(1, os.path.join(os.path.dirname(__file__), '..' , 'src'))

from unittest.mock import patch

import ijolly
import jollypy

def test_default_printing_strategy_is_hex():
    interactive_jolly = ijolly.InteractiveJolly(None)

    assert type(interactive_jolly.integer_print_strategy) is ijolly.IntegerToHexString

def test_switch_to_decimal():
    interactive_jolly = ijolly.InteractiveJolly(None)
    interactive_jolly.switch_to_decimal()

    assert type(interactive_jolly.integer_print_strategy) is ijolly.IntegerToDecimalString

def test_switch_to_hexadecimal():
    interactive_jolly = ijolly.InteractiveJolly(None)
    interactive_jolly.switch_to_decimal()
    interactive_jolly.switch_to_hexadecimal()

    assert type(interactive_jolly.integer_print_strategy) is ijolly.IntegerToHexString

@patch('jollypy.JollyVM')
def test_next(VMMock):
    vm = jollypy.JollyVM()
    interactive_jolly = ijolly.InteractiveJolly(vm)

    interactive_jolly.next()

    assert vm.execute_instruction.call_count == 1

@patch('jollypy.JollyVM')
def test_next_count(VMMock):
    vm = jollypy.JollyVM()
    interactive_jolly = ijolly.InteractiveJolly(vm)

    interactive_jolly.next(5)

    assert vm.execute_instruction.call_count == 5

