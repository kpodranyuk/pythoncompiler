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
public class None extends Value{
    
    @Override
    public boolean equals(Object other) {
        return other instanceof None;
    }

    @Override
    public java.lang.String toString() {
        return "None";
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
