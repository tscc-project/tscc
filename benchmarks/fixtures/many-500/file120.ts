interface Item120 { id: number; name: string; }
type Key120 = string | number;
export function make120(id: number, name: string): Item120 {
  const value: Item120 = {id, name};
  return value as Item120;
}
export const item120: Item120 = make120(120, "item-120");
