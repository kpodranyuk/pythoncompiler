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

    public boolean value;

    public Boolean(boolean val) {
        this.value = val;
    }

    @Override
    public Value add(Value other) {
        if (other instanceof Boolean) {
            return new Integer((this.toIntBool()) + (((Boolean) other).toIntBool()));
        } else if (other instanceof Integer) {
            return new Integer((this.toIntBool()) + (((Integer) other).value));
        }
        throw new Error("Operation not allowed with these types.");
    }
    
    @Override
    public Value sub(Value other) {
        if (other instanceof Integer) {
            return new Integer(this.toIntBool() - ((Integer) other).value);
        } else if(other instanceof Boolean) {
            return new Integer(this.toIntBool() - ((Boolean) other).toIntBool());
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public Value mul(Value other) {
        if ((other instanceof Boolean)) {
            return new Integer((this.toIntBool()) * (((Boolean) other).toIntBool()));
        } else if (other instanceof Integer) {
            return new Integer((this.toIntBool()) * (((Integer) other).value));
        }
        throw new Error("Operation not allowed with these types.");
    }
    
    @Override
    public Value pow(Value other) {
        if (other instanceof Integer) {
            return new Integer((int) Math.pow(this.toIntBool(), ((Integer) other).value));
        } else if(other instanceof Boolean) {
            return new Integer((int) Math.pow(this.toIntBool(), ((Boolean) other).toIntBool()));
        }
        throw new Error("Operation not allowed with these types.");
    }
    
    @Override
    public Value div(Value other) {
        if (other instanceof Integer) {
            return new Integer(this.toIntBool() / ((Integer) other).value);
        } else if(other instanceof Boolean) {
            int number = ((Boolean) other).toIntBool();
            if(number==1) {
                return new Integer(this.toIntBool() / number);
            } else {
                throw new Error("Division by zero.");
            }
        }
        throw new Error("Operation not allowed with these types.");
    }
    
    @Override
    public Value mod(Value other) {
        if (other instanceof Integer) {
            return new Integer(this.toIntBool() % ((Integer) other).value);
        } else if(other instanceof Boolean) {
            int number = ((Boolean) other).toIntBool();
            if(number==1) {
                return new Integer(this.toIntBool() % number);
            } else {
                throw new Error("Division by zero.");
            }
        }
        throw new Error("Operation not allowed with these types.");
    }
    

    @Override
    public Value less(Value other) {
        if(other instanceof Boolean) {
            Boolean bol = (Boolean)other;
            if(this.toIntBool() < bol.toIntBool()) {
                return new Boolean(true);
            } else {
                return new Boolean(false);
            }     
        } else {
            throw new Error("Operation not allowed with these types.");
        }
    }
    
    
    @Override
    public int toIntBool() {
        return this.value ? 1 : 0;
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
    public int hashCode() {
        return this.value ? 1 : 0;
    }

}
