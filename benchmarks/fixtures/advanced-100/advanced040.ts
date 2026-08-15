namespace N40 {
  export const base: number = 40;
  export function add(x: number): number { return base + x; }
}
function over40(x: number): number;
function over40(x: string): string;
function over40(x: number | string): number | string { return x; }
abstract class A40 { abstract f(x: number): number; }
class B40 extends A40 { override f(x: number): number { return x + 1; } }
enum E40 { A = 5 / 2, B, C = 2 ** 3, D }
const angle40 = <number>(40 + 1);
const check40 = {x: angle40} satisfies {x: number};
console.log(N40.add(1), over40(40), new B40().f(1), E40.B, E40.D, check40.x);
