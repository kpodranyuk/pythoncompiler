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
    public int toIntBool() {
        if(this.value.size() == 0)
            return 0;
        return 1;
    }
  
    @Override
    public Value less(Value other) {
        if(other instanceof List) {
            List list = (List)other;
            int sizeList;
            // Находим наименьший из листов(размер)
            if(this.value.size() > list.value.size()) {
                sizeList=list.value.size();
            } else {
                sizeList=this.value.size();
            }
            
            // Если листы равны, то результатом операции будет false
            if(this.equals(list)) {
                return new Boolean(false);
            }
            
            // Проходим по всем элементам наименьшего листа
            // Результатом лог выражения будет первое значение true или false
            for(int i=0; i<sizeList; i++) {
                if((this.value.get(i).less(list.value.get(i))).toIntBool()==1 && !this.value.get(i).equals(list.value.get(i))) {
                    return new Boolean(true);
                }
            }
            
            return new Boolean(false);
            
        } else {
            throw new Error("Operation not allowed with these types.");
        }
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
        List list = new List();
        list.value.add(this);
        return list;
    }

    @Override
    public int hashCode() {
        return 0;
    }
    
}
