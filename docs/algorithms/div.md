# Division
This documentation explains how the **Bitwise Long Division** (Binary Long Division) is implemented.

## Definitions
* **Dividend (`data`):** The number being divided
* **Divisor (`divisor`):** The number you are dividing by
* **Quotient (`quotient`):** This is the result number. How many times the divisor fits entirely into the dividend.
* **Remainder / Modulo (`remaining`):** The leftover amount that is strictly less than the divisor.
* **Dividend Mask (`dividendMask`):** The active "working remainder" used dynamically within the long division loop to evaluate the current subset of bits.

## The Basic Concept
Binary Long Division works exactly like the long division taught in elementary school, but it is simpler because the quotient at any step can only ever be `1` or `0` (it either fits, or it doesn't).
1. Isolate the highest piece of the dividend.
2. Check if the divisor fits into it.
3. If it fits, write `1` to the quotient, subtract the divisor, and "bring down" the next bit.
4. If it doesn't fit, write `0` to the quotient, don't subtract, and "bring down" the next bit.

**Quick Binary Example `7 / 2`:**
- `7` (`111`)
- `2` (`10`)

**The Setup:**
```text
             [Quotient]
           _____________
Divisor: 10 | 1   1   1  <-- Dividend
              ^       ^
             MSB     LSB
         (Highest) (Lowest)
```

**Step 1: Isolate the highest bit (MSB)**
```text
           0          <-- 10 does not fit into 1. Write 0 to quotient.
         _______
     10 |  1 1 1
         - 0          <-- No subtraction occurs.
         ---
           1          <-- Working remainder is 1.
```

**Step 2: Bring down the next bit**
```text
           0 1        <-- 10 fits into 11! Write 1 to quotient.
         _______
     10 |  1 1 1
           . |
           1 1        <-- Bring down the 1. Working remainder is now 11.
         - 1 0        <-- Subtract divisor (10).
         -----
             1        <-- New working remainder is 1.
```

**Step 3: Bring down the last bit (LSB)**
```text
           0 1 1      <-- 10 fits into 11! Write 1 to quotient.
         _______
     10 |  1 1 1
             . |
             1 1      <-- Bring down the final 1. Working remainder is 11.
           - 1 0      <-- Subtract divisor (10).
           -----
               1      <-- Final Remainder!
```

**Result:**
The quotient is `001` (`3` in decimal). The leftover value at the bottom is our remainder: **`1`**.
*(7 / 2 = 3 R 1)*.


## Implementation
The division algorithm is implement within 5 steps.
### 1. Initialization
1. **Division by Zero:** If the divisor is `0`, we abort and default the quotient and remainder to `0`.
2. **Finding the MSB** Using the `getStartBitIndex` function we get the MSB and assign it to `initialDividendIndex`. This ensures that we don't process any ghost zeros because the highest byte could look something like this `00010001`.
3. **Preallocation:** Because the exact bit-length of the quotient correlates directly to the `initialDividendIndex`, the `quotient` vector is pre-allocated to the maximum required size (`(initialDividendIndex / 8) + 1`).

### 2. Working Remainder (`dividendMask`)
`dividendMask` is initialized by taking a value of `0`, shifting it left by 1, and "bringing down" the absolute highest bit from the **dividend**.

### 3. Bitwise Evaluation Loop
In a `while` loop, bit by bit is processed from `initialDividendIndex` down to `-1`.
For every bit position, the mathematical power index (`currentQBitIndex`) is evaluated.

At each step we check if the `dividendMask` (working remainder) is $\ge$ the `divisor`.

**Case A: The Mask is $\ge$ the Divisor**
* The divisor "fits" inside the working remainder.
* A `1` is written to the exact corresponding bit inside the pre-allocated `quotient` vector using a bitwise OR: `quotient[byteIndex] |= (1 << bitIndex)`.
* The `divisor` is subtracted from the `dividendMask`.
* If there are still bits left in the dividend, the `dividendMask` is shifted left by 1, and the *next* bit from the dividend is brought down into the LSB of the mask.

**Case B: The Mask is $<$ the Divisor**
* The divisor does not fit. The quotient bit remains `0` (its default pre-allocated state).
* No subtraction occurs.
* The `dividendMask` is simply shifted left by 1, and the *next* bit from the dividend is brought down to increase the value of the mask for the next loop iteration.

### 4. Modulo (The Remainder)
Because this is integer division, there is often a fractional remainder. The `div` function accepts an optional pointer to a `remaining` vector (`Bytes* remaining`).
When the loop finishes processing the final bit (`dividendIndex < 0`), whatever mathematical value is left inside the `dividendMask` is exactly the modulo. If the pointer is provided, the mask is copied into it.

*(Note: Because of this architecture, evaluating `A % B` requires the exact same computational effort as `A / B`. Therefore, if both the quotient and remainder are needed, they are extracted simultaneously to halve CPU cycles).*

### 5. Final Normalization
Even though pre-allocation is tightly bound to the `initialDividendIndex`, the final quotient might have leading zeros depending on the magnitude of the divisor. The `div` function concludes by stripping any trailing zero-bytes from the little-endian vector to maintain strict `BigInt` normalization guarantees.

---

## Visualization: `13 / 3`
* **Dividend:** `13` (`1101`)
* **Divisor:** `3` (`0011`)

**Initialization:**
* `getStartBitIndex(1101)` returns `3` (the 0-based index of the highest `1` bit).
* `dividendMask` (the working remainder) is initialized to `0000`.
* `quotient` is pre-allocated and initialized to `0000`.

### Step 1: Processing Bit Index 3
* **Action:** Shift `dividendMask` left by 1, and bring down Bit 3 of the dividend (`1`).
* **Mask Status:** `dividendMask` becomes `0001` (Decimal: 1).
* **Comparison:** Is `0001` $\ge$ `0011` (Divisor)? $\rightarrow$ **FALSE**
* **Result:**
    * Divisor does not fit. No subtraction.
    * Bit 3 of `quotient` remains `0`.
    * **Current Quotient:** `0000`
    * **Current Mask:** `0001`

### Step 2: Processing Bit Index 2
* **Action:** Shift `dividendMask` left by 1 (`0001` $\rightarrow$ `0010`), and bring down Bit 2 of the dividend (`1`).
* **Mask Status:** `dividendMask` becomes `0011` (Decimal: 3).
* **Comparison:** Is `0011` $\ge$ `0011` (Divisor)? $\rightarrow$ **TRUE**
* **Result:**
    * Divisor fits!
    * Set Bit 2 of `quotient` to `1` using bitwise OR.
    * Subtract divisor from mask: `0011` - `0011` = `0000`.
    * **Current Quotient:** `0100`
    * **Current Mask:** `0000`

### Step 3: Processing Bit Index 1
* **Action:** Shift `dividendMask` left by 1 (`0000` $\rightarrow$ `0000`), and bring down Bit 1 of the dividend (`0`).
* **Mask Status:** `dividendMask` becomes `0000` (Decimal: 0).
* **Comparison:** Is `0000` $\ge$ `0011` (Divisor)? $\rightarrow$ **FALSE**
* **Result:**
    * Divisor does not fit. No subtraction.
    * Bit 1 of `quotient` remains `0`.
    * **Current Quotient:** `0100`
    * **Current Mask:** `0000`

### Step 4: Processing Bit Index 0 (LSB)
* **Action:** Shift `dividendMask` left by 1 (`0000` $\rightarrow$ `0000`), and bring down Bit 0 of the dividend (`1`).
* **Mask Status:** `dividendMask` becomes `0001` (Decimal: 1).
* **Comparison:** Is `0001` $\ge$ `0011` (Divisor)? $\rightarrow$ **FALSE**
* **Result:**
    * Divisor does not fit. No subtraction.
    * Bit 0 of `quotient` remains `0`.
    * **Current Quotient:** `0100`
    * **Current Mask:** `0001`

### Final Output Evaluation:
The loop terminates because we have processed bit index `0`.
1. **The Quotient:** The pre-allocated quotient vector holds `0100`, which mathematically evaluates to **`4`**.
2. **The Remainder:** The `dividendMask` is left holding `0001`, which mathematically evaluates to **`1`**. If a pointer for the modulo was provided, this value is safely copied over.

**Conclusion:** `1101 / 0011` = `0100` with a remainder of `0001` ($13 / 3 = 4\text{ R }1$).
