interface Item473 { id: number; name: string; }
type Key473 = string | number;
export function make473(id: number, name: string): Item473 {
  const value: Item473 = {id, name};
  return value as Item473;
}
export const item473: Item473 = make473(473, "item-473");
