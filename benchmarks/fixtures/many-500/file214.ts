interface Item214 { id: number; name: string; }
type Key214 = string | number;
export function make214(id: number, name: string): Item214 {
  const value: Item214 = {id, name};
  return value as Item214;
}
export const item214: Item214 = make214(214, "item-214");
