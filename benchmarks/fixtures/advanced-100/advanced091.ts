namespace N91 {
  export const base: number = 91;
  export function add(x: number): number { return base + x; }
}
function over91(x: number): number;
function over91(x: string): string;
function over91(x: number | string): number | string { return x; }
abstract class A91 { abstract f(x: number): number; }
class B91 extends A91 { override f(x: number): number { return x + 1; } }
enum E91 { A = 5 / 2, B, C = 2 ** 3, D }
const angle91 = <number>(91 + 1);
const check91 = {x: angle91} satisfies {x: number};
console.log(N91.add(1), over91(91), new B91().f(1), E91.B, E91.D, check91.x);
