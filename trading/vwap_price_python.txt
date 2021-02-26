from typing import Tuple

import math
import pytest

def vwap_price(required_size: int, sizes: Tuple[float, ...], prices: Tuple[float, ...]) -> float:
    """
    Get the VWAP price for a size from the given profile, or NaN if a price
    cannot be calculated.
    """
    if not prices or not sizes:
        if required_size == 0:
            return 0.0
        else:
            return float('NaN')

    cnt = 0
    result = 0.0

    for p, s in zip(prices, sizes):
        if required_size and cnt == required_size:
            break
        else:
            if(required_size and cnt + s > required_size):
                result += p * (required_size - cnt)
                cnt = required_size
            else:
                result += p * s
                cnt += s

    if cnt < required_size:
        return float('NaN')

    if cnt == 0:
        return 0.0
    else:
        return result / cnt


@pytest.mark.skip(reason="interview only")
@pytest.mark.parametrize(
    ["required_size", "prices",       "sizes",                "expected_vwap_price"],
    [
     [0,              [],                   [],               0.0],
     [1,              [],                   [],               float('NaN')],
     [0,              [1.0],                [1],              1.0],
     [1,              [1.0],                [1],              1.0],
     [1,              [0.0],                [1],              0.0],
     [2,              [0.0, 1.0, 3.0],      [1, 1, 5],        0.5],
     [5,              [0.0, 1.0, 3.0],      [1, 2, 5],        1.6],
     [8,              [0.0, 1.0, 3.0],      [1, 2, 5],        2.125],
     [9,              [0.0, 1.0, 3.0],      [1, 2, 5],        float('NaN')],
    ])
def test_vwap_price(required_size, prices, sizes, expected_vwap_price):
    if math.isnan(expected_vwap_price):
        assert math.isnan(vwap_price(required_size, sizes, prices))
    else:
        assert vwap_price(required_size, sizes, prices) == expected_vwap_price
