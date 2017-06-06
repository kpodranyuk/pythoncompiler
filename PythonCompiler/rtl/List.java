/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package rtl;

import java.util.ArrayList;
import java.util.Collection;

/**
 *
 * @author Никита
 */
public class List extends Value {

    public ArrayList<Value> value;
    
    public List() {
        this.value = new ArrayList<Value>();
    }

    
    @Override
    public Value add(Value other) {
        if(other instanceof List) {
            List newList = new List();
            newList.value.addAll(this.value);
            newList.value.add((Value)other);
        
            return newList;
        }
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public Value moreOrEq(Value other) {
        return super.moreOrEq(other); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public Value lessOrEq(Value other) {
        return super.lessOrEq(other); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public Value more(Value other) {
        return super.more(other); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public Value less(Value other) {
        return super.less(other); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public Value eq(Value other) {
        return super.eq(other); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public int toIntBool() {
        if(this.value.size() == 0)
            return 0;
        return 1;
    }

    @Override
    public Value and(Value other) {
        if(this.toIntBool() == 0 || other.toIntBool() == 0)
            return new Boolean(false);
        return new Boolean(true);
    }

    @Override
    public Value or(Value other) {
        if(this.toIntBool() == 0 && other.toIntBool() == 0)
            return new Boolean(false);
        return new Boolean(true);
    }

    @Override
    public Value not() {
        if(this.toIntBool() == 0)
            return new Boolean(true);
        return new Boolean(false);
    }
    
    
    @Override
    public boolean equals(Object paramObject) {
        Value obj = (Value) paramObject;
        if(obj instanceof List) {
            List list = (List) obj;
            if(list.value.size() != this.value.size()) {
                return false;
            } else {
                for(int i=0; i<list.value.size(); i++) {
                    if(!list.value.get(i).equals(this.value.get(i)))
                        return false;
                }
                return true;
            }
        }
        return false;
    }

    @Override
    public java.lang.String toString() {
        java.lang.String res = "[\n";
        for (Value val : this.value) {
          res = res + val.toString();
          res = res + ",\n";
        }
        return res + "]";
    }

    @Override
    public Value clone() {
        return new None();
    }

    @Override
    public int hashCode() {
        return 0;
    }
    
}
