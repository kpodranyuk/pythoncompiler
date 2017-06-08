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
public class String extends Value {
    public java.lang.String value = new java.lang.String();

    public String(java.lang.String val) {
        if (val == null) {
            throw new Error("Attempt to construct string with null initializer.");
        }
        this.value = val;
    }

    public String(char val) {
        this.value = ("" + val);
    }

    public Value charAt(Integer val) {
        return new String(this.value.charAt(val.value - 1));
    }

    @Override
    public Value add(Value other) {
        if ((other instanceof String)) {
            return new String(this.value + ((String) other).value);
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public Value mul(Value other) {
        if ((other instanceof Integer)) {
            java.lang.String res = new java.lang.String("");
            int count = ((Integer) other).value;
            for(int i=0; i<count; i++) {
                res = res + this.value;
            }
            return new String(res);
        }
        throw new Error("Operation not allowed with these types.");
    }
    
    @Override
    public int toIntBool() {
        if(this.value.isEmpty())
            return 0;
        return 1;
    }
    
    @Override
    public Value less(Value other) {
        if(other instanceof String) {
            String str = (String)other;
            if(this.value.compareTo(str.value) < 0) {
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
        if ((other instanceof String)) {
            return this.value.equals(((String) other).value);
        }
        return false;
    }

    @Override
    public java.lang.String toString() {
        return new java.lang.String(this.value);
    }

    @Override
    public Value clone() {
        return new String(this.value);
    }

    @Override
    public int hashCode() {
        return this.value.hashCode();
    }
}
