/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package rtl;

import java.io.PrintStream;
import java.util.Scanner;
/**
 * Класс основной библиотеки RTL для использования
 * Служит оберткой для вызовов методов
 * Реализует системные функции (ввод/вывод)
 * @author Katie
 */
public class Lib {
    private static final java.lang.String EXCEPTION_TYPE_MISMATCH = "[ERROR] Type mismatch.";

    public static Value toNumber(Value e) {
        if ((e instanceof Integer)) {   // Целое возвращает себя
            return e;
        }
        if ((e instanceof Float)) {     // Флоат - только целая часть
            return valueFromInt((int)((Float) e).value);
        }
        if ((e instanceof String)) {    // Строка - перевод
            return new Integer(java.lang.Integer.parseInt(((String) e).value));
        }
        if ((e instanceof Boolean)) {   // Булеан - 0 или 1
            return new Integer(toIntBool(((Boolean) e)));
        }
        if ((e instanceof None)) {      // None - 0
            return new Integer(0);
        }
        return valueFromNone();
    }

    public static Value tonumberBase(Value e, Value base) {
        //throw new Error("[ERROR] tonumber: base not supported.");
        /*if ((e instanceof Integer)) {
            return e;
        }*/
        if (!(e instanceof String)) {   // e - может быть только строкой
            throw new Error("Can't use int(x,base) function with first argument that is not a string.");
        }
        if(!(base instanceof Integer)){ // base - может быть только целым числом
            throw new Error("Can't use int(x,base) function with second argument that is not an integer.");
        }
        // Мы принимаем все основания кроме 0, 1 и больше 36
        int baseI = ((Integer)base).value;
        if(baseI<2||baseI>36)
            throw new Error("Incorrect base in int(x,base) call: "+base.toString()+".");
        if ((e instanceof String)) {
            return new Integer(java.lang.Integer.parseInt(((String) e).value, ((Integer)base).value));
        }
        return valueFromNone();
    }

    

    public static void print(Value val) {
        if(val==null)
            throw new Error("Can't print NULL operand");
        System.out.println(val.toString());
    }

    public static Value input(Value toPrint) {
        // Печатаем передаваемое содержимое
        if (!(toPrint instanceof None))
            Lib.print(toPrint);
        // Создаем сканнер
        Scanner scan = new Scanner(System.in);
        // Считываем строку
        java.lang.String s = scan.next();
        // Кладем строку на стек
        return valueFromString(s);
    }

    public static Value valueFromNone() {
        return new None();
    }
    
    public static Value valueFromInt(int i) {
        return new Integer(i);
    }

    public static Value valueFromFloat(float f) {
        return new Float(f);
    }

    public static Value valueFromString(java.lang.String s) {
        return new String(s);
    }

    public static Value valueFromList() {
        return new List();
    }
    
    public static Value valueFromBoolean(int val) {
        return new Boolean(val!=0);
    }
    
    public static Value toString(Value e) {
        return valueFromString(e.toString());
    }

    public static Value sub(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.sub(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.sub(m2);
        } else if (m2 instanceof Iterator) {
            return m1.sub(((Iterator)m2).currentValue);
        }
        return m1.sub(m2);
    }

    public static Value add(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.add(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.add(m2);
        } else if (m2 instanceof Iterator) {
            return m1.add(((Iterator)m2).currentValue);
        }
        return m1.add(m2);
    }

    public static Value mul(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.mul(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.mul(m2);
        } else if (m2 instanceof Iterator) {
            return m1.mul(((Iterator)m2).currentValue);
        } 
        return m1.mul(m2);
    }
    
    public static Value pow(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.pow(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.pow(m2);
        } else if (m2 instanceof Iterator) {
            return m1.pow(((Iterator)m2).currentValue);
        }
        return m1.pow(m2);
    }

    public static Value div(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.div(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.div(m2);
        } else if (m2 instanceof Iterator) {
            return m1.div(((Iterator)m2).currentValue);
        }
        return m1.div(m2);
    }
    
    public static Value intDiv(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.intDiv(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.intDiv(m2);
        } else if (m2 instanceof Iterator) {
            return m1.intDiv(((Iterator)m2).currentValue);
        }
        return m1.intDiv(m2);
    }

    public static Value mod(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.mod(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.mod(m2);
        } else if (m2 instanceof Iterator) {
            return m1.mod(((Iterator)m2).currentValue);
        }
        return m1.mod(m2);
    }

    public static int toIntBool(Value m1) {
        if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.toIntBool();
        }
        return m1.toIntBool();
    }

    public static Value eq(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.eq(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.eq(m2);
        } else if (m2 instanceof Iterator) {
            return m1.eq(((Iterator)m2).currentValue);
        }
        return m1.eq(m2);
    }

    public static Value less(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.less(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.less(m2);
        } else if (m2 instanceof Iterator) {
            return m1.less(((Iterator)m2).currentValue);
        }
        return m1.less(m2);
    }

    public static Value more(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.more(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.more(m2);
        } else if (m2 instanceof Iterator) {
            return m1.more(((Iterator)m2).currentValue);
        }
        return m1.more(m2);
    }

    public static Value lessOrEq(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.lessOrEq(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.lessOrEq(m2);
        } else if (m2 instanceof Iterator) {
            return m1.lessOrEq(((Iterator)m2).currentValue);
        }
        return m1.lessOrEq(m2);
    }

    public static Value moreOrEq(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.moreOrEq(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.moreOrEq(m2);
        } else if (m2 instanceof Iterator) {
            return m1.moreOrEq(((Iterator)m2).currentValue);
        }
        return m1.moreOrEq(m2);
    }

    public static Value notEq(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.eq(((Iterator)m2).currentValue).not();
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.eq(m2).not();
        } else if (m2 instanceof Iterator) {
            return m1.eq(((Iterator)m2).currentValue).not();
        }
        return m1.eq(m2).not();
    }

    public static Value not(Value m) {
        if(m instanceof Iterator) {
            return ((Iterator)m).currentValue.not();
        }
        return m.not();
    }

    public static Value or(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.or(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.or(m2);
        } else if (m2 instanceof Iterator) {
            return m1.or(((Iterator)m2).currentValue);
        }
        return m1.or(m2);
    }

    public static Value and(Value m1, Value m2) {
        if ((m1 instanceof Iterator) && (m2 instanceof Iterator)) {
            return ((Iterator)m1).currentValue.and(((Iterator)m2).currentValue);
        } else if(m1 instanceof Iterator) {
            return ((Iterator)m1).currentValue.and(m2);
        } else if (m2 instanceof Iterator) {
            return m1.and(((Iterator)m2).currentValue);
        }
        return m1.and(m2);
    }

    public static Value ListGet(Value list, Value index) {
        Value listM = list;
        Value indexM = index;
        
        if(list instanceof Iterator) {
            listM = ((Iterator)list).currentValue;
        } 
        if (index instanceof Iterator) {
            indexM = ((Iterator)index).currentValue;
        }
        
        if (((listM instanceof List)) && ((indexM instanceof Integer))) {
            int newIndex = ((Integer) indexM).value;
            if(newIndex<0) {
                newIndex=flipIndex(newIndex, ((List) listM).value.size());
            }
            if(newIndex >= ((List) listM).value.size()) {
                throw new Error("Going beyond the array.");
            }
            
            return ((List)listM).value.get(newIndex);
        } else if (((listM instanceof String)) && ((indexM instanceof Integer))) {
            int newIndex = ((Integer) indexM).value;
            if(newIndex<0) {
                newIndex=flipIndex(newIndex, ((String) listM).value.length());
            }
            if(newIndex >= ((List) listM).value.size()) {
                throw new Error("Going beyond the array.");
            }
            
            return ((String)listM).charAt(new Integer(newIndex+1));
        }
        throw new Error("The index must be an integer and the operation must occur over the array.");
    }

    public static void ListSet(Value list, Value index, Value val) {
        Value listM = list;
        Value indexM = index;
        Value valM = val;
        
        if(list instanceof Iterator) {
            listM = ((Iterator)list).currentValue;
        } 
        if (index instanceof Iterator) {
            indexM = ((Iterator)index).currentValue;
        }
        if (val instanceof Iterator) {
            val = ((Iterator)val).currentValue;
        }
        
        
        if(!(listM instanceof List)){
            throw new Error("Can't use ListSet operation without List.");
        } 
        if(!(indexM instanceof Integer)) {
            throw new Error("Can't use ListSet operation when [index] isn't integer.");
        }
        
        int newIndex = ((Integer) indexM).value;
        if(newIndex<0) {
            newIndex=flipIndex(newIndex, ((List) listM).value.size());
        }
        if(newIndex >= ((List) listM).value.size()) {
            throw new Error("Going beyond the array.");
        }
        ((List) listM).value.set(newIndex, valM);
    }

    public static Value ListAppend(Value list, Value val) {
        Value listM = list;
        Value valM = val;
        
        if(list instanceof Iterator) {
            listM = ((Iterator)list).currentValue;
        } 
        if (val instanceof Iterator) {
            valM = ((Iterator)val).currentValue;
        }
        
        
        if ((listM instanceof List)) {
            ((List) listM).value.add(valM);
        } else {
            throw new Error("The operation is not applicable to an array.");
        }
        return valueFromNone();
    }
    
    public static Value ListAddInitialize(Value list, Value val) {
        Value listM = list;
        Value valM = val;
        
        if(list instanceof Iterator) {
            listM = ((Iterator)list).currentValue;
        } 
        if (val instanceof Iterator) {
            valM = ((Iterator)val).currentValue;
        }
        
        
        if ((listM instanceof List)) {
            ((List) listM).value.add(valM);
        } else {
            throw new Error("The operation is not applicable to an array.");
        }
        return listM;
    }
    
    public static Value ListRemove(Value list, Value val) {
        Value listM = list;
        Value valM = val;
        
        if(list instanceof Iterator) {
            listM = ((Iterator)list).currentValue;
        } 
        if (val instanceof Iterator) {
            valM = ((Iterator)val).currentValue;
        }
        
        
        if ((listM instanceof List)) {
            if(valM instanceof None || valM instanceof Integer || valM instanceof Boolean || valM instanceof String || valM instanceof Float) {
               ((List) listM).value.remove(valM); 
            } else {
                throw new Error("Value must be a standard type.");
            }
        } else {
            throw new Error("The operation is not applicable to an array.");
        }
        return valueFromNone();
    }
    
    public static int flipIndex(int index, int size) {
        int newIndex = index;
        newIndex+=size;
        return newIndex;
    }
    
    public static Value newIterator(Value listOrStr, Value iter) {
        if(!(listOrStr instanceof List) && !(listOrStr instanceof String)){
            throw new Error("No list or string in FOR.");
        } 
        
        if(iter==null) {
            iter = new Iterator();
            ((Iterator) iter).newIterator(listOrStr);
        } else if(iter!=null) {
            if(iter instanceof Iterator) {
                return ((Iterator) iter).newIterator(listOrStr);
            } else {
                iter = new Iterator();
                return ((Iterator) iter).newIterator(listOrStr);
            }
        }
        
        return iter;
    }
    
    public static Value next(Value iter) {
        if(!(iter instanceof Iterator)) {
            throw new Error("No iterator.");
        }

        return ((Iterator)iter).next();
    }
    
    public static int hasNext(Value iter) {
        if(!(iter instanceof Iterator)) {
            throw new Error("No iterator.");
        }
        
        return ((Iterator)iter).hasNext();
    }
    
    public static Value valueIterator(Value iter) {
        if(!(iter instanceof Iterator)) {
            throw new Error("No iterator.");
        }
        
        return ((Iterator)iter).currentValue;
    }
    
    public static void setValueIterator(Value iter, Value value) {
        if(!(iter instanceof Iterator)) {
            throw new Error("No iterator.");
        }
        
        ((Iterator)iter).currentValue=value;
    }
    
}
