import sys
import os

sys.path.insert(1, os.path.join(os.path.dirname(__file__), '..' , 'src'))

from unittest.mock import MagicMock

import ijolly

def test_end_address_from_address_0():
    watcher = ijolly.MemoryWatcher("foo", 0, 5)

    assert watcher.end_address() == 4

def test_end_address_from_address_42():
    watcher = ijolly.MemoryWatcher("foo", 42, 5)

    assert watcher.end_address() == 46

def test_extract_0_5():
    watcher = ijolly.MemoryWatcher("foo", 0, 5)
    memory = [1, 2, 3, 4, 5, 6, 7, 8]

    assert watcher.extract(memory) == [1, 2, 3, 4, 5]

def test_extract_42_5():
    watcher = ijolly.MemoryWatcher("foo", 42, 5)
    memory = [x for x in range(0, 100)]

    assert watcher.extract(memory) == [42, 43, 44, 45, 46]

def test_extract_memory_to_string_0_5_decimal():
    watcher = ijolly.MemoryWatcher("foo", 0, 5)
    interactive_jolly = ijolly.InteractiveJolly(None)
    interactive_jolly.switch_to_decimal()
    interactive_jolly.memory = MagicMock(return_value=[1, 2, 3, 4, 5, 6, 7, 8])

    assert watcher.extract_memory_to_string(interactive_jolly) == '1 2 3 4 5'

def test_extract_memory_to_string_42_5_decimal():
    watcher = ijolly.MemoryWatcher("foo", 42, 5)
    interactive_jolly = ijolly.InteractiveJolly(None)
    interactive_jolly.switch_to_decimal()
    interactive_jolly.memory = MagicMock(return_value=[x for x in range(0, 100)])

    assert watcher.extract_memory_to_string(interactive_jolly) == '42 43 44 45 46'

def test_extract_memory_to_string_0_5_hexadecimal():
    watcher = ijolly.MemoryWatcher("foo", 0, 5)
    interactive_jolly = ijolly.InteractiveJolly(None)
    interactive_jolly.switch_to_hexadecimal()
    interactive_jolly.memory = MagicMock(return_value=[1, 2, 3, 4, 5, 6, 7, 8])

    assert watcher.extract_memory_to_string(interactive_jolly) == '01 02 03 04 05'

def test_extract_memory_to_string_42_5_hexadecimal():
    watcher = ijolly.MemoryWatcher("foo", 42, 5)
    interactive_jolly = ijolly.InteractiveJolly(None)
    interactive_jolly.switch_to_hexadecimal()
    interactive_jolly.memory = MagicMock(return_value=[x for x in range(0, 100)])

    assert watcher.extract_memory_to_string(interactive_jolly) == '2a 2b 2c 2d 2e'

def test_to_user_string_0_5_hexadecimal():
    watcher = ijolly.MemoryWatcher("foo", 0, 5)
    interactive_jolly = ijolly.InteractiveJolly(None)
    interactive_jolly.switch_to_hexadecimal()
    interactive_jolly.memory = MagicMock(return_value=[1, 2, 3, 4, 5, 6, 7, 8])

    assert watcher.to_user_string(interactive_jolly) == '"foo" 0x00000000: 01 02 03 04 05'

def test_to_user_string_42_5_hexadecimal():
    watcher = ijolly.MemoryWatcher("foo", 42, 5)
    interactive_jolly = ijolly.InteractiveJolly(None)
    interactive_jolly.switch_to_hexadecimal()
    interactive_jolly.memory = MagicMock(return_value=[x for x in range(0, 100)])

    assert watcher.to_user_string(interactive_jolly) == '"foo" 0x0000002a: 2a 2b 2c 2d 2e'