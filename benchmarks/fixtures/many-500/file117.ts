interface Item117 { id: number; name: string; }
type Key117 = string | number;
export function make117(id: number, name: string): Item117 {
  const value: Item117 = {id, name};
  return value as Item117;
}
export const item117: Item117 = make117(117, "item-117");
