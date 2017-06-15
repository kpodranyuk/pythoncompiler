/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package rtl;

/**
 * Значение числа с плавающей точкой
 *
 * @author Katie
 */
public class Float extends Value{

    public float value;

    public Float(float val) {
        this.value = val;
    }

    public boolean equals(Object other) {
        if ((other instanceof Integer)) {
            return this.value == ((Integer) other).value;
        }
        if ((other instanceof Float)) {
            return this.value == ((Float) other).value;
        }
        return false;
    }

    public java.lang.String toString() {
        return "" + this.value;
    }

    public Value clone() {
        return new Float(this.value);
    }

     @Override
    public Value add(Value other) {
        if ((other instanceof Float)) {
            return new Float(this.value + ((Float) other).value);
        }
        if (other instanceof Integer) {
            return new Float(this.value + ((Integer) other).value);
        } else if(other instanceof Boolean) {
            return new Float(this.value + ((Boolean) other).toIntBool());
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public Value sub(Value other) {
        if ((other instanceof Float)) {
            return new Float(this.value - ((Float) other).value);
        }
        if (other instanceof Integer) {
            return new Float(this.value - ((Integer) other).value);
        } else if(other instanceof Boolean) {
            return new Float(this.value - ((Boolean) other).toIntBool());
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public Value mul(Value other) {
        if ((other instanceof Float)) {
            return new Float(this.value * ((Float) other).value);
        }
        if (other instanceof Integer) {
            return new Float(this.value * ((Integer) other).value);
        } else if(other instanceof Boolean) {
            return new Float(this.value * ((Boolean) other).toIntBool());
        }
        throw new Error("Operation not allowed with these types.");
    }
    
    @Override
    public Value pow(Value other) {
        if (other instanceof Integer) {
            return new Float((float)Math.pow(this.value, ((Integer) other).value));
        } else if(other instanceof Boolean) {
            return new Float((float) Math.pow(this.value, ((Boolean) other).toIntBool()));
        } else if(other instanceof Float) {
            return new Float((float) Math.pow(this.value, ((Float) other).value));
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public Value div(Value other) {
        if ((other instanceof Float)) {
            return new Float(this.value / ((Float) other).value);
        }
        if (other instanceof Integer) {
            return new Float(this.value / ((Integer) other).value);
        } else if(other instanceof Boolean) {
            int number = ((Boolean) other).toIntBool();
            if(number==1) {
                return new Float(this.value / number);
            } else {
                throw new Error("Division by zero.");
            }
        }
        throw new Error("Operation not allowed with these types.");
    }
    
    @Override
     public Value intDiv(Value other) {
        if ((other instanceof Float)) {
            return new Integer((int)this.value / ((Integer) other).value);
        }
        if (other instanceof Integer) {
            return new Integer((int)this.value / ((Integer) other).value);
        } else if(other instanceof Boolean) {
            int number = ((Boolean) other).toIntBool();
            if(number==1) {
                return new Integer((int)this.value / number);
            } else {
                throw new Error("Division by zero.");
            }
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public Value mod(Value other) {
        if ((other instanceof Float)) {
            return new Float(this.value % ((Float) other).value);
        }
        if (other instanceof Integer) {
            return new Float(this.value % ((Integer) other).value);
        } else if(other instanceof Boolean) {
            int number = ((Boolean) other).toIntBool();
            if(number==1) {
                return new Float(this.value % number);
            } else {
                throw new Error("Division by zero.");
            }
        }
        throw new Error("Operation not allowed with these types.");
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
        } else if(other instanceof Boolean) { 
            Boolean in = (Boolean)other;
            if(this.value < in.toIntBool()) {
                return new Boolean(true);
            } else {
                return new Boolean(false);
            }
        } else if(other instanceof Float) { 
            Float in = (Float)other;
            if(this.value < in.value) {
                return new Boolean(true);
            } else {
                return new Boolean(false);
            }
        } else {
            throw new Error("Operation not allowed with these types.");
        }
    }

    public int hashCode() {
        return (int) this.value;
    }
}
