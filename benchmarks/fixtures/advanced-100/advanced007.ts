namespace N7 {
  export const base: number = 7;
  export function add(x: number): number { return base + x; }
}
function over7(x: number): number;
function over7(x: string): string;
function over7(x: number | string): number | string { return x; }
abstract class A7 { abstract f(x: number): number; }
class B7 extends A7 { override f(x: number): number { return x + 1; } }
enum E7 { A = 5 / 2, B, C = 2 ** 3, D }
const angle7 = <number>(7 + 1);
const check7 = {x: angle7} satisfies {x: number};
console.log(N7.add(1), over7(7), new B7().f(1), E7.B, E7.D, check7.x);
