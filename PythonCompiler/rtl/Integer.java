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
    public Value pow(Value other) {
        if ((other instanceof Integer)) {
            return new Integer((int) Math.pow(this.value, ((Integer) other).value));
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
    public int toIntBool() {
        if(this.value != 0)
            return 1;
        return 0;
    }
    
    @Override
    public Value less(Value other) {
        if(other instanceof Integer) {
            Integer in = (Integer)other;
            if(this.value < in.value) {
                return new Boolean(true);
            } else {
                return new Boolean(false);
            }
        } else {
            throw new Error("Operation not allowed with these types.");
        }
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
    public java.lang.String toString() {
        return "" + this.value;
    }

    @Override
    public Value clone() {
        return new Integer(this.value);
    }
    
    @Override
    public int hashCode() {
        return this.value;
    }
}
