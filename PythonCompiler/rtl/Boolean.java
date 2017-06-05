/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package rtl;

/**
 *
 * @author Katie
 */
public class Boolean extends Value {

    boolean value;

    public Boolean(boolean val) {
        this.value = val;
    }

    @Override
    public boolean equals(Object other) {
        if ((other instanceof Boolean)) {
            return this.value == ((Boolean) other).value;
        }
        return false;
    }

    @Override
    public java.lang.String toString() {
        return this.value ? "true" : "false";
    }

    @Override
    public Value clone() {
        return new Boolean(this.value);
    }

    @Override
    public Value add(Value other) {
        if ((other instanceof Boolean)) {
            return new Boolean((this.value) || (((Boolean) other).value));
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public Value mul(Value other) {
        if ((other instanceof Boolean)) {
            return new Boolean((this.value) && (((Boolean) other).value));
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public int hashCode() {
        return this.value ? 1 : 0;
    }

    @Override
    public int toIntBool() {
        return this.value ? 1 : 0;
    }

    @Override
    public Value not() {
        return new Boolean(!this.value);
    }

    @Override
    public Value and(Value m) {
        return new Boolean((this.value) && (m.toIntBool() != 0));
    }

    @Override
    public Value or(Value m) {
        return new Boolean((this.value) && (m.toIntBool() != 0));
    }

}
