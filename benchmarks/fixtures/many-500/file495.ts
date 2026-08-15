interface Item495 { id: number; name: string; }
type Key495 = string | number;
export function make495(id: number, name: string): Item495 {
  const value: Item495 = {id, name};
  return value as Item495;
}
export const item495: Item495 = make495(495, "item-495");
