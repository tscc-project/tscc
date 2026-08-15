namespace N26 {
  export const base: number = 26;
  export function add(x: number): number { return base + x; }
}
function over26(x: number): number;
function over26(x: string): string;
function over26(x: number | string): number | string { return x; }
abstract class A26 { abstract f(x: number): number; }
class B26 extends A26 { override f(x: number): number { return x + 1; } }
enum E26 { A = 5 / 2, B, C = 2 ** 3, D }
const angle26 = <number>(26 + 1);
const check26 = {x: angle26} satisfies {x: number};
console.log(N26.add(1), over26(26), new B26().f(1), E26.B, E26.D, check26.x);
