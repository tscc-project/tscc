namespace N31 {
  export const base: number = 31;
  export function add(x: number): number { return base + x; }
}
function over31(x: number): number;
function over31(x: string): string;
function over31(x: number | string): number | string { return x; }
abstract class A31 { abstract f(x: number): number; }
class B31 extends A31 { override f(x: number): number { return x + 1; } }
enum E31 { A = 5 / 2, B, C = 2 ** 3, D }
const angle31 = <number>(31 + 1);
const check31 = {x: angle31} satisfies {x: number};
console.log(N31.add(1), over31(31), new B31().f(1), E31.B, E31.D, check31.x);
