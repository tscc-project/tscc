interface Item18 { id: number; name: string; }
type Key18 = string | number;
export function make18(id: number, name: string): Item18 {
  const value: Item18 = {id, name};
  return value as Item18;
}
export const item18: Item18 = make18(18, "item-18");
