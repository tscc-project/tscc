namespace N42 {
  export const base: number = 42;
  export function add(x: number): number { return base + x; }
}
function over42(x: number): number;
function over42(x: string): string;
function over42(x: number | string): number | string { return x; }
abstract class A42 { abstract f(x: number): number; }
class B42 extends A42 { override f(x: number): number { return x + 1; } }
enum E42 { A = 5 / 2, B, C = 2 ** 3, D }
const angle42 = <number>(42 + 1);
const check42 = {x: angle42} satisfies {x: number};
console.log(N42.add(1), over42(42), new B42().f(1), E42.B, E42.D, check42.x);
