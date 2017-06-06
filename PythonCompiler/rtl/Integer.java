/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package rtl;

/**
 * Целочисленное значение
 * @author Katie
 */
public class Integer extends Value{
    public int value;

    public Integer(int val) {
        this.value = val;
    }

    @Override
    public boolean equals(Object other) {
        if ((other instanceof Integer)) {
            return this.value == ((Integer) other).value;
        }
        /*if ((other instanceof Float)) {
            return this.value == ((Float) other).value;
        }*/
        return false;
    }

    @Override
    public Value more(Value other) {
        if ((other instanceof Integer)) {
            return new Boolean(this.value > ((Integer) other).value);
        }
        /*if ((other instanceof Float)) {
            return new Boolean(this.value > ((Float) other).value);
        }*/
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public java.lang.String toString() {
        return "" + this.value;
    }

    @Override
    public Value clone() {
        return new Integer(this.value);
    }

    @Override
    public Value add(Value other) {
        /*if ((other instanceof Float)) {
            return new Float(this.value + ((Float) other).value);
        }*/
        if ((other instanceof Integer)) {
            return new Integer(this.value + ((Integer) other).value);
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public Value sub(Value other) {
        /*if ((other instanceof Float)) {
            return new Float(this.value - ((Float) other).value);
        }*/
        if ((other instanceof Integer)) {
            return new Integer(this.value - ((Integer) other).value);
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public Value mul(Value other) {
        /*if ((other instanceof Float)) {
            return new Float(this.value * ((Float) other).value);
        }*/
        if ((other instanceof Integer)) {
            return new Integer(this.value * ((Integer) other).value);
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public Value div(Value other) {
        /*if ((other instanceof Float)) {
            return new Float(this.value / ((Float) other).value);
        }*/
        if ((other instanceof Integer)) {
            return new Integer(this.value / ((Integer) other).value);
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public Value mod(Value other) {
        /*if ((other instanceof Float)) {
            return new Float(this.value % ((Float) other).value);
        }*/
        if ((other instanceof Integer)) {
            return new Integer(this.value % ((Integer) other).value);
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public int hashCode() {
        return this.value;
    }

    @Override
    public int toIntBool() {
        if(this.value != 0)
            return 1;
        return 0;
    }

    @Override
    public Value less(Value other) {
        if ((other instanceof Integer)) {
            return new Boolean(this.value < ((Integer) other).value);
        }
        return new Boolean(false);
    }

    @Override
    public Value eq(Value other) {
        if ((other instanceof Integer)) {
            return new Boolean(this.value == ((Integer) other).value);
        }
        return new Boolean(false);
    }

    @Override
    public Value not() {
        return new Boolean(this.value != 0).not();
    }

    @Override
    public Value and(Value m) {
        return new Boolean(this.value != 0).and(m);
    }

    @Override
    public Value or(Value m) {
        return new Boolean(this.value != 0).or(m);
    }
}
