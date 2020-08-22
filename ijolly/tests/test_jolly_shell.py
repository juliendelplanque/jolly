import sys
import os

sys.path.insert(1, os.path.join(os.path.dirname(__file__), '..' , 'src'))

from unittest.mock import MagicMock, PropertyMock

import ijolly

def test_parse_input_pc():
    interactive_jolly = ijolly.InteractiveJolly(None)
    type(interactive_jolly).pc_address = PropertyMock(return_value=42)
    shell = ijolly.JollyShell(interactive_jolly)
    
    assert shell.parse_input("pc") == 42

def test_parse_input_42_dex():
    interactive_jolly = ijolly.InteractiveJolly(None)
    shell = ijolly.JollyShell(interactive_jolly)
    
    assert shell.parse_input("42") == 42

def test_parse_input_42_hex():
    interactive_jolly = ijolly.InteractiveJolly(None)
    shell = ijolly.JollyShell(interactive_jolly)
    
    assert shell.parse_input("0x2a") == 42

def test_parse_input_string():
    interactive_jolly = ijolly.InteractiveJolly(None)
    shell = ijolly.JollyShell(interactive_jolly)
    
    assert shell.parse_input("foo") == "foo"