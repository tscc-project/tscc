interface Item451 { id: number; name: string; }
type Key451 = string | number;
export function make451(id: number, name: string): Item451 {
  const value: Item451 = {id, name};
  return value as Item451;
}
export const item451: Item451 = make451(451, "item-451");
