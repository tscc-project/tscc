interface Item436 { id: number; name: string; }
type Key436 = string | number;
export function make436(id: number, name: string): Item436 {
  const value: Item436 = {id, name};
  return value as Item436;
}
export const item436: Item436 = make436(436, "item-436");
