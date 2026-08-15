namespace N16 {
  export const base: number = 16;
  export function add(x: number): number { return base + x; }
}
function over16(x: number): number;
function over16(x: string): string;
function over16(x: number | string): number | string { return x; }
abstract class A16 { abstract f(x: number): number; }
class B16 extends A16 { override f(x: number): number { return x + 1; } }
enum E16 { A = 5 / 2, B, C = 2 ** 3, D }
const angle16 = <number>(16 + 1);
const check16 = {x: angle16} satisfies {x: number};
console.log(N16.add(1), over16(16), new B16().f(1), E16.B, E16.D, check16.x);
