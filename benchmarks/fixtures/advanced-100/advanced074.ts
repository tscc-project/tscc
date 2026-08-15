namespace N74 {
  export const base: number = 74;
  export function add(x: number): number { return base + x; }
}
function over74(x: number): number;
function over74(x: string): string;
function over74(x: number | string): number | string { return x; }
abstract class A74 { abstract f(x: number): number; }
class B74 extends A74 { override f(x: number): number { return x + 1; } }
enum E74 { A = 5 / 2, B, C = 2 ** 3, D }
const angle74 = <number>(74 + 1);
const check74 = {x: angle74} satisfies {x: number};
console.log(N74.add(1), over74(74), new B74().f(1), E74.B, E74.D, check74.x);
