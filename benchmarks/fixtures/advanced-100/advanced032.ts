namespace N32 {
  export const base: number = 32;
  export function add(x: number): number { return base + x; }
}
function over32(x: number): number;
function over32(x: string): string;
function over32(x: number | string): number | string { return x; }
abstract class A32 { abstract f(x: number): number; }
class B32 extends A32 { override f(x: number): number { return x + 1; } }
enum E32 { A = 5 / 2, B, C = 2 ** 3, D }
const angle32 = <number>(32 + 1);
const check32 = {x: angle32} satisfies {x: number};
console.log(N32.add(1), over32(32), new B32().f(1), E32.B, E32.D, check32.x);
