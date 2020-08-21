import sys
import os

sys.path.insert(1, os.path.join(os.path.dirname(__file__), '..' , 'src'))

import ijolly

def test_IntegerToHexString_to_string_0_padding_0():
    converter = ijolly.IntegerToHexString()
    assert converter.to_string(0, 0) == '0x0'

def test_IntegerToHexString_to_string_42_padding4():
    converter = ijolly.IntegerToHexString()
    assert converter.to_string(42, 4) == '0x002a'

def test_IntegerToDecimalString_to_string_0_ignorepadding():
    converter = ijolly.IntegerToDecimalString()
    assert converter.to_string(0, 42) == '0'

def test_IntegerToDecimalString_to_string_42():
    converter = ijolly.IntegerToDecimalString()
    assert converter.to_string(42) == '42'