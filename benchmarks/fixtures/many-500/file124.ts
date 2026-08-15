interface Item124 { id: number; name: string; }
type Key124 = string | number;
export function make124(id: number, name: string): Item124 {
  const value: Item124 = {id, name};
  return value as Item124;
}
export const item124: Item124 = make124(124, "item-124");
