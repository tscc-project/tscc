namespace N23 {
  export const base: number = 23;
  export function add(x: number): number { return base + x; }
}
function over23(x: number): number;
function over23(x: string): string;
function over23(x: number | string): number | string { return x; }
abstract class A23 { abstract f(x: number): number; }
class B23 extends A23 { override f(x: number): number { return x + 1; } }
enum E23 { A = 5 / 2, B, C = 2 ** 3, D }
const angle23 = <number>(23 + 1);
const check23 = {x: angle23} satisfies {x: number};
console.log(N23.add(1), over23(23), new B23().f(1), E23.B, E23.D, check23.x);
