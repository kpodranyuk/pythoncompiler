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
public class Function extends Value{
    
    @Override
    public boolean equals(Object other) {
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public java.lang.String toString() {
        return "function: @" + System.identityHashCode(this);
    }

    @Override
    public Value clone() {
        throw new Error("Operation not allowed with these types.");
    }

    @Override
    public int hashCode() {
        return 0;
    }
    
}
