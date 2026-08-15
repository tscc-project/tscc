interface Item71 { id: number; name: string; }
type Key71 = string | number;
export function make71(id: number, name: string): Item71 {
  const value: Item71 = {id, name};
  return value as Item71;
}
export const item71: Item71 = make71(71, "item-71");
