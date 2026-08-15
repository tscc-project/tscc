interface Item156 { id: number; name: string; }
type Key156 = string | number;
export function make156(id: number, name: string): Item156 {
  const value: Item156 = {id, name};
  return value as Item156;
}
export const item156: Item156 = make156(156, "item-156");
