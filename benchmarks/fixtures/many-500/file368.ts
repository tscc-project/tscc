interface Item368 { id: number; name: string; }
type Key368 = string | number;
export function make368(id: number, name: string): Item368 {
  const value: Item368 = {id, name};
  return value as Item368;
}
export const item368: Item368 = make368(368, "item-368");
