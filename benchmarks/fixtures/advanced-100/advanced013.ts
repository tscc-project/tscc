namespace N13 {
  export const base: number = 13;
  export function add(x: number): number { return base + x; }
}
function over13(x: number): number;
function over13(x: string): string;
function over13(x: number | string): number | string { return x; }
abstract class A13 { abstract f(x: number): number; }
class B13 extends A13 { override f(x: number): number { return x + 1; } }
enum E13 { A = 5 / 2, B, C = 2 ** 3, D }
const angle13 = <number>(13 + 1);
const check13 = {x: angle13} satisfies {x: number};
console.log(N13.add(1), over13(13), new B13().f(1), E13.B, E13.D, check13.x);
