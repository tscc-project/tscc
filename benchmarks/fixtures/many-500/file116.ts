interface Item116 { id: number; name: string; }
type Key116 = string | number;
export function make116(id: number, name: string): Item116 {
  const value: Item116 = {id, name};
  return value as Item116;
}
export const item116: Item116 = make116(116, "item-116");
