interface Item179 { id: number; name: string; }
type Key179 = string | number;
export function make179(id: number, name: string): Item179 {
  const value: Item179 = {id, name};
  return value as Item179;
}
export const item179: Item179 = make179(179, "item-179");
