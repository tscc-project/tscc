interface Item149 { id: number; name: string; }
type Key149 = string | number;
export function make149(id: number, name: string): Item149 {
  const value: Item149 = {id, name};
  return value as Item149;
}
export const item149: Item149 = make149(149, "item-149");
