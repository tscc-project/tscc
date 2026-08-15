namespace N56 {
  export const base: number = 56;
  export function add(x: number): number { return base + x; }
}
function over56(x: number): number;
function over56(x: string): string;
function over56(x: number | string): number | string { return x; }
abstract class A56 { abstract f(x: number): number; }
class B56 extends A56 { override f(x: number): number { return x + 1; } }
enum E56 { A = 5 / 2, B, C = 2 ** 3, D }
const angle56 = <number>(56 + 1);
const check56 = {x: angle56} satisfies {x: number};
console.log(N56.add(1), over56(56), new B56().f(1), E56.B, E56.D, check56.x);
