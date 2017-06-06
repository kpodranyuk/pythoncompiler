/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package rtl;

import java.io.PrintStream;
//import java.util.ArrayList;
import java.util.Scanner;
/**
 * Класс основной библиотеки RTL для использования
 * Служит оберткой для вызовов методов
 * Реализует системные функции (ввод/вывод)
 * @author Katie
 */
public class Lib {
    private static final java.lang.String EXCEPTION_TYPE_MISMATCH = "[ERROR] Type mismatch.";

    /*public static Value lassert(Value v, Value msg) {
        if (v.equals(new Boolean(false))) {
            error(msg);
            return null;
        }
        return new String("assertion " + msg.toString() + " passed");
    }

    public static Value lassert(Value v) {
        return lassert(v, new String("assertion failed!"));
    }

    public static void error(Value msg, Value level) {
        throw new Error(msg.toString());
    }

    public static void error(Value msg) {
        error(msg, new Integer(0));
    }*/

    public static Value tonumber(Value e) {
        if ((e instanceof Integer)) {
            return e;
        }
        if ((e instanceof String)) {
            return new Integer(java.lang.Integer.parseInt(((String) e).value));
        }
        return new None();
    }

    public static Value tonumber(Value e, Value base) {
        throw new Error("[ERROR] tonumber: base not supported.");
    }

    public static Value tostring(Value e) {
        return new String(e.toString());
    }

    public static void print(Value val) {
        System.out.println(val.toString());
    }

    public static Value ioread(Value format) {
        Scanner scanner = new Scanner(System.in);
        java.lang.String vf = null;
        if ((format instanceof String)) {
            vf = ((String) format).value;
            /*if (vf == "*n") {
                return new Float(scanner.nextDouble());
            }*/
            if (vf == "*l") {
                return new String(scanner.nextLine());
            }
            throw new Error("[ERROR] io.read: Quantifier not supported.");
        }
        if ((format instanceof Integer)) {
            return new String(scanner.next(".{" + ((Integer) format).value + "}"));
        }
        throw new Error("[ERROR] Type mismatch.");
    }

    public static Value mixedFromNone() {
        return new None();
    }
    
    public static Value mixedFromInt(int i) {
        return new Integer(i);
    }

    /*public static Value mixedFromFloat(float f) {
        return new Float(f);
    }*/

    public static Value mixedFromString(java.lang.String s) {
        return new String(s);
    }

    /*public static Value mixedFromList() {
        return new List();
    }*/

    public static Value sub(Value m1, Value m2) {
        return m1.sub(m2);
    }

    public static Value add(Value m1, Value m2) {
        return m1.add(m2);
    }

    public static Value mul(Value m1, Value m2) {
        return m1.mul(m2);
    }

    public static Value div(Value m1, Value m2) {
        return m1.div(m2);
    }

    public static Value mod(Value m1, Value m2) {
        return m1.mod(m2);
    }

    public static int toIntBool(Value m1) {
        return m1.toIntBool();
    }

    public static Value eq(Value m1, Value m2) {
        return m1.eq(m2);
    }

    public static Value less(Value m1, Value m2) {
        return m1.less(m2);
    }

    public static Value more(Value m1, Value m2) {
        return m1.more(m2);
    }

    public static Value lessOrEq(Value m1, Value m2) {
        return m1.lessOrEq(m2);
    }

    public static Value moreOrEq(Value m1, Value m2) {
        return m1.moreOrEq(m2);
    }

    public static Value notEq(Value m1, Value m2) {
        return m1.eq(m2).not();
    }

    public static Value not(Value m) {
        return m.not();
    }

    public static Value or(Value m1, Value m2) {
        return m1.or(m2);
    }

    public static Value and(Value m1, Value m2) {
        return m1.and(m2);
    }

    /*public static Mixed ListGet(Mixed list, Mixed index) {
        if (((list instanceof List)) && ((index instanceof Integer))) {
            return (Mixed) ((List) list).value.get(((Integer) index).value);
        }
        return mixedFromUndefined();
    }

    public static void ListSet(Mixed list, Mixed index, Mixed val) {
        if (((list instanceof List)) && ((index instanceof Integer))) {
            ((List) list).value.set(((Integer) index).value, val);
        }
    }

    public static Mixed ListAdd(Mixed list, Mixed val) {
        if ((list instanceof List)) {
            ((List) list).value.add(val);
            return list;
        }
        return mixedFromUndefined();
    }*/
}