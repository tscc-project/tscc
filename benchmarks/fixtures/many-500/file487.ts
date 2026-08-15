interface Item487 { id: number; name: string; }
type Key487 = string | number;
export function make487(id: number, name: string): Item487 {
  const value: Item487 = {id, name};
  return value as Item487;
}
export const item487: Item487 = make487(487, "item-487");
