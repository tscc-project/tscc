namespace N30 {
  export const base: number = 30;
  export function add(x: number): number { return base + x; }
}
function over30(x: number): number;
function over30(x: string): string;
function over30(x: number | string): number | string { return x; }
abstract class A30 { abstract f(x: number): number; }
class B30 extends A30 { override f(x: number): number { return x + 1; } }
enum E30 { A = 5 / 2, B, C = 2 ** 3, D }
const angle30 = <number>(30 + 1);
const check30 = {x: angle30} satisfies {x: number};
console.log(N30.add(1), over30(30), new B30().f(1), E30.B, E30.D, check30.x);
